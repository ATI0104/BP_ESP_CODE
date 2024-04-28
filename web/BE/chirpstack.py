import base64
from sqlalchemy.orm import sessionmaker
import sqlalchemy as db
from sqlalchemy.orm import DeclarativeBase
from sqlalchemy import create_engine
from datetime import datetime
from config import connection_string

database_name = "chirpstack"

engine = create_engine(connection_string + database_name)
session = sessionmaker(bind=engine)


class Base(DeclarativeBase):
    pass


class application(Base):
    __tablename__ = "application"
    id = db.Column(db.Uuid, primary_key=True)


class device_profile(Base):
    __tablename__ = "device_profile"
    id = db.Column(db.Uuid, primary_key=True)


def get_application_id():
    with session() as s:
        app = s.query(application).first()
        if app:
            return app.id
        else:
            return None


def get_device_profile_id():
    with session() as s:
        dp = s.query(device_profile).first()
        if dp:
            return dp.id
        else:
            return None


application_id = get_application_id()
device_profile_id = get_device_profile_id()


class device(Base):
    __tablename__ = "device"
    dev_eui = db.Column(db.BINARY, primary_key=True)
    application_id = db.Column(
        db.Uuid, db.ForeignKey("application.id"), default=application_id
    )
    device_profile_id = db.Column(
        db.Uuid, db.ForeignKey("device_profile.id"), default=device_profile_id
    )
    created_at = db.Column(db.DateTime, default=datetime.now)
    updated_at = db.Column(db.DateTime, default=datetime.now, onupdate=datetime.now)
    last_seen_at = db.Column(db.DateTime, default=None)
    scheduler_run_after = db.Column(db.DateTime, default=None)
    name = db.Column(db.String)
    description = db.Column(db.String, default="")
    external_power_source = db.Column(db.Boolean, default=False)
    battery_level = db.Column(db.Numeric, default=None)
    margin = db.Column(db.Integer, default=None)
    dr = db.Column(db.SmallInteger, default=None)
    latitude = db.Column(db.Numeric, default=None)
    longitude = db.Column(db.Numeric, default=None)
    altitude = db.Column(db.Numeric, default=None)
    dev_addr = db.Column(db.BINARY, default=None)
    enabled_class = db.Column(db.CHAR, default="A")
    skip_fcnt_check = db.Column(db.Boolean, default=False)
    is_disabled = db.Column(db.Boolean, default=False)
    tags = db.Column(db.JSON, default={})
    variables = db.Column(db.JSON, default={})
    join_eui = db.Column(db.BINARY)


class device_keys(Base):
    __tablename__ = "device_keys"
    dev_eui = db.Column(db.BINARY, primary_key=True)
    created_at = db.Column(db.DateTime, default=datetime.now)
    updated_at = db.Column(db.DateTime, default=datetime.now, onupdate=datetime.now)
    nwk_key = db.Column(db.BINARY)
    app_key = db.Column(db.BINARY, default=base64.b64decode("AAAAAAAAAAAAAAAAAAAAAA=="))
    dev_nonces = db.Column(db.ARRAY(db.Integer), default=lambda: [])
    join_nonce = db.Column(db.Integer, default=0)


def add_device(dev_eui: bytes, join_eui: bytes, app_key: bytes, name: str):
    try:
        with session() as s:
            d = s.query(device).filter(device.dev_eui == dev_eui).first()
            if d:
                return False
            new_device = device(
                dev_eui=dev_eui,
                join_eui=join_eui,
                name=name,
            )
            s.add(new_device)
            new_device_key = device_keys(
                dev_eui=dev_eui,
                nwk_key=app_key,
            )
            s.add(new_device_key)
            s.commit()
            return True
    except Exception as e:
        print(e)
        return False
