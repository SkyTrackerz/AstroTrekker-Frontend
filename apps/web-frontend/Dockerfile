FROM node:12.10.0-alpine
EXPOSE 8080
EXPOSE 8888

WORKDIR /app

# Configure default user environment, re-use node user/group
RUN chown -R node /app
USER node
RUN id node

COPY package.json yarn.lock ./
RUN yarn

CMD yarn run dev
