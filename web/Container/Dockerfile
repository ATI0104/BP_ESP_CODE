# Use Fedora as the base image
FROM fedora:38
# Install nginx
RUN dnf -y update && \
    dnf -y install nginx && \
    dnf clean all
COPY ./nginx.conf /etc/nginx/nginx.conf
# Expose port 8080 for further routing
EXPOSE 8080
# run nginx
CMD ["nginx", "-g", "daemon off;"]
