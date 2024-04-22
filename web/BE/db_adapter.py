import threading
import sqlalchemy as db
from sqlalchemy import create_engine
from sqlalchemy.orm import DeclarativeBase
from sqlalchemy.orm import joinedload
from sqlalchemy.orm import sessionmaker
from sqlalchemy.orm import relationship
from sqlalchemy import text
from datetime import datetime, timedelta,timezone
from cachetools import func
from config import connection_string, database_name


chirpstack_db_name = "chirpstack"
maintenance_db_name = "postgres"
engine = create_engine(connection_string + database_name)
session = sessionmaker(bind=engine)

class Base(DeclarativeBase):
    pass


class Panels(Base):
    __tablename__ = "panels"
    id = db.Column(db.LargeBinary, primary_key=True)
    name = db.Column(db.String)
    created_at = db.Column(db.DateTime, default=datetime.now)
    updated_at = db.Column(db.DateTime, default=datetime.now, onupdate=datetime.now)
    # Relationship to load panel data
    panel_data = relationship("Panel_data", back_populates="panel")

    def to_dict(self):
        self.dev_eui = self.id.hex()
        # Converts object to a dictionary
        return {
            "deveui": self.dev_eui,
            "name": self.name,
            "created_at": self.created_at.replace(tzinfo=timezone.utc).strftime(
                "%m/%d/%Y %I:%M:%S %p %Z"
            ),
            "updated_at": self.updated_at.replace(tzinfo=timezone.utc).strftime(
                "%m/%d/%Y %I:%M:%S %p %Z"
            ),
            "panel_data": sorted(
                [data.to_dict() for data in self.panel_data],
                key=lambda x: x["created_at"],
            ),
        }


class Panel_data(Base):
    __tablename__ = "panel_data"
    id = db.Column(db.Integer, primary_key=True)
    panel_id = db.Column(db.LargeBinary, db.ForeignKey("panels.id"))
    pv_voltage = db.Column(db.Float)
    pv_current = db.Column(db.Float)
    battery_voltage = db.Column(db.Float)
    bypass_state = db.Column(db.Boolean)
    created_at = db.Column(db.DateTime, default=datetime.now)
    report_interval = db.Column(db.Integer)
    # Back reference to associate with Panels
    panel = relationship("Panels", back_populates="panel_data")

    def to_dict(self):
        # Converts object to a dictionary
        return {
            "id": self.id,
            "pv_voltage": self.pv_voltage / 1000.0,
            "pv_current": self.pv_current / 1000.0,
            "bypass_state": bool(self.bypass_state),
            "created_at": self.created_at.replace(tzinfo=timezone.utc).strftime(
                "%m/%d/%Y %I:%M:%S %p %Z"
            ),
            "report_interval": self.report_interval,
        }


def dev_eui_to_bytearray(dev_eui):
    return bytes.fromhex(dev_eui)


def get_panels():
    with session() as s:
        panels = s.query(Panels).options(joinedload(Panels.panel_data)).all()
        return [panel.to_dict() for panel in panels]


def get_panel_by_deveui(dev_eui):
    with session() as s:
        panel = (
            s.query(Panels)
            .filter(Panels.deveui == dev_eui_to_bytearray(dev_eui))
            .first()
        )
        return panel


def add_panel(panel):
    with session() as s:
        s.add(panel)
        s.commit()
        return panel.id


@func.ttl_cache(ttl=24 * 60 * 60)
def remove_old_events():
    with session() as s:
        s.query(Panel_data).filter(
            Panel_data.created_at < datetime.now() - timedelta(days=7)
        ).delete()
        s.commit()
    return 1


def add_event(dev_eui, data: dict, created_at=datetime.now()):
    try:
        panel_data = Panel_data(
            panel_id=dev_eui_to_bytearray(dev_eui),
            pv_voltage=data["pv_voltage"] if data["pv_voltage"] > 0.0 else 0.0,
            pv_current=data["pv_current"] if data["pv_current"] > 0.0 else 0.0,
            battery_voltage=data["battery_voltage"],
            bypass_state=data["bypass_state"],
            report_interval=data["report_interval"],
            created_at=created_at,
        )
        with session() as s:
            s.add(panel_data)
            s.commit()
            threading.Thread(target=remove_old_events).start()
            return True
    except Exception as e:
        print(f"Failed to add event: {e}")
        return False


def create_database(connection_string):
    # Creates the database if it does not exist
    engine = create_engine(connection_string + maintenance_db_name)
    conn = engine.connect()
    # Begin transaction
    conn.execute(text("COMMIT"))  # Ensures any pending transaction is committed
    try:
        conn.execute(text(f"CREATE DATABASE {database_name}"))
        print(f"Database '{database_name}' created successfully.")
    except:
        print(f"Database '{database_name}' already exists.")
        # exit(1)

    conn.close()
    engine.dispose()


if __name__ == "__main__":
    create_database(connection_string)
    Base.metadata.create_all(engine)
