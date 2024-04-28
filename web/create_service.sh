#!/bin/sh
CWDIR=$(pwd)
echo "Creating service..."
echo "#!/bin/sh /etc/rc.common

# Start priority, valid range is 0-99.
START=99
# Stop priority, valid range is 0-99.
STOP=10

start() {
    echo "Starting webserver service"
    ${CWDIR}/run.sh
}

stop() {
    echo "Stopping webserver service"
    ${CWDIR}/stop.sh
}
" > /etc/init.d/webserver

chmod +x /etc/init.d/webserver
/etc/init.d/webserver enable
