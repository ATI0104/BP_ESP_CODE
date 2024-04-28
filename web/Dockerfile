# Use Alpine as the base image
FROM alpine:latest

# Install Nginx, Node.js, and Python
RUN apk add --no-cache bash nginx nodejs npm python3 py3-pip py3-virtualenv

# Setup Nginx for running foreground
RUN mkdir -p /run/nginx

# Copy frontend and backend code into the image
COPY FE /frontend
COPY BE /backend

# Install frontend dependencies and build the frontend
WORKDIR /frontend
RUN npm install && \
    npm run build

# Copy the built frontend files to the serving directory
RUN mkdir -p /www && \
    cp -r dist/* /www/

# Set the appropriate permissions for Nginx to access /www
RUN chmod -R 755 /www && \
    chown -R nginx:nginx /www

# Set up the backend environment
WORKDIR /backend
RUN python3 -m venv venv && \
    . venv/bin/activate && \
    pip install -r requirements.txt

# Copy the Nginx configuration
COPY ./nginx.conf /etc/nginx/nginx.conf

# Expose the required port
EXPOSE 8081

# Define the command to run Nginx and the backend simultaneously
CMD ["sh", "-c", "nginx; source venv/bin/activate && exec python3 web.py"]

