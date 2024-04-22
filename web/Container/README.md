## A simple container which routes traffic from the BE and FE to one endpoint. This is used to combine the two services and to prevent CORS related issues. This was mainly used during development, but I kept it in case someone wants to run the webserver on a different device

### Usage:

- Run ./build.sh to build the container
- Run ./run.sh to run the container
- Start the BE and FE locally
- Navigate to http://localhost:8080 to see the FE
- Navigate to http://localhost:8080/api/ to see the BE
- Run ./stop.sh to stop the container


