# Monitoring and control of photovoltaic systems

This folder contains the files required for the web application.
The BE directory contains the backend code and the FE directory contains the frontend code.
## Installation
- Inside the BE directory, run the following commands:
  - bash ./init.sh
  - source venv/bin/activate
  - python3 web.py
  - The backend is running on the configured port
- Inside the FE directory, run the following commands:
  - npm install
  - npm run dev or npm run build - npm run dev launches the application in debug mode, which means that you can access it on http://localhost:5173. npm run build compiles the application and you can find the files in the /dist directory. You need to serve these files using a web server like Nginx.
- After both of the services are running you need to setup a reverse proxy to combine the frontend and backend. You can use Nginx for this purpose. By default, the FE is sending request to the /api/\* endpoint.

## Configuration

- The backend is running on port 8888 by default. You can change this by modifying the web.py file. You will need to edit the nginx.conf file to accomodate for these changes
- It's expected that the backend will be run on the same device as the database server and the mqtt broker. If this is not the case you will need to modify the config.py file inside the BE directory
- The frontend is sending requests to the /api/\* endpoint. You can change this by modifying the configStore.ts file.
