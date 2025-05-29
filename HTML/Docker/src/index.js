const app = require ('express')();

app.get('/', (request, response) => {
  response.send('Hello World!');
});

const port = process.env.PORT || 3000;

app.listen(port, () => {
  console.log(`Server is running on port ${port}`);
});