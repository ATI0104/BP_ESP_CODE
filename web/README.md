# Monitoring and control of photovoltaic systems

This folder contains the files required for the web application.
The BE directory contains the backend code and the FE directory contains the frontend code.
I'm including a Dockerfile and 2 run scripts which speeds up deploying and running the application.

There are 2 ways to deploy this software:

1. Using Podman [Recommended]

- This is the easiest way to deploy the application.
- You just need to run the run.sh script and all the dependencies will be automatically installed. The application will be available at http://localhost:8081

2. Local Installation

- Inside the BE directory, run the following commands:
  - bash ./init.sh
  - source venv/bin/activate
  - python3 web.py
  - Now the backend is running
- Inside the FE directory, run the following commands:
  - npm install
  - npm run dev or npm run build - npm run dev launches the application in debug mode, which means that you can access it on http://localhost:5173. npm run build compiles the application and you can find the files in the /dist directory. You need to serve these files using a web server like Nginx.
- After both of the services are running you need to setup a reverse proxy to combine the frontend and backend. You can use Nginx for this purpose. By default, the FE is sending request to the /api/\* endpoint.

Configuration:

- The backend is running on port 8888 by default. You can change this by modifying the web.py file. You will need to edit the nginx.conf file to accomodate for these changes
- It's expected that the backend will be run on the same device as the database server and the mqtt broker. If this is not the case you will need to modify the config.py file inside the BE directory
- The frontend is sending requests to the /api/\* endpoint. You can change this by modifying the configStore.ts file.

## Deploying on ChirpStack Gateway OS

- Install podman on the Raspberry Pi
- Copy this directory to the Rapsberry Pi
- Run `sh ./create_service.sh` to install the application as a service
- Run `/etc/init.d/webserver start`
