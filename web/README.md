This folder contains the files required for the web application. 
The BE directory contains the backend code and the FE directory contains the frontend code.
I'm including a Dockerfile and 2 run scripts which helps deploy the application. 

There are 2 ways to deploy this software:
1. Using Podman [Recommended]
- This is the easiest way to deploy the application.
- You just need to run the run.sh script and all the dependencies will be automatically installed. The application will be available at http://localhost:8081

2. Local Installation
 - Inside the BE directory, run the following commands:
    - bash ./init.sh
    - python3 web.py
    - Now the backend is running
 - Inside the FE directory, run the following commands:
    - npm install
    - npm run dev or npm run build - npm run dev launches the application in debug mode, which means that you can access it on http://localhost:5173. npm run build compiles the application and you can find the files in the /dist directory. You need to serve these files using a web server like Nginx.
- After both of the services are running you need to setup a reverse proxy to combine the frontend and backend. You can use Nginx for this purpose. By default, the FE is sending request to the /api/* endpoint.

Configuration:
- The backend is running on port 8888 by default. You can change this by modifying the web.py file.
- It's expected that the backend will be run on the same device as the database server. You can change the database server address by modifying the config.py file.
- The frontend is sending requests to the /api/* endpoint. You can change this by modifying the configStore.ts file.