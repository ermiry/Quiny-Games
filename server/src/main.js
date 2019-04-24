const result = require ('dotenv').config ({ path: './config/keys.env' });
// if (result.error) throw result.error

const express = require ('express');
const mongoose = require ('mongoose');
const bodyParser = require ('body-parser');
const passport = require ('passport');

const app = express ();

// Body parser middleware
app.use (bodyParser.urlencoded ({ extended: false }));
app.use (bodyParser.json());


// DB Config
// const db = require('./config/keys').mongoURI;

// Connect to MongoDB
// mongoose
//     .connect ('mongodb://localhost/test', { useNewUrlParser: true })
//     .then (() => console.log ('MongoDB Connected'))
//     .catch (err => console.log (err));

// passport
// FIXME:
// app.use (passport.initialize ());
// require ('../config/passport')(passport);

/*** ROUTES ***/

const users = require ('./routes/api/users');
const games = require ('./routes/api/games');
const quiny = require ('./routes/api/quiny');

app.get ("/", (req, res) => { res.json ({ msg: "Quiny Games "}); });

app.use ('/api/users', users);
app.use ('/api/games', games);

app.use ('/api/quiny', quiny);

// catch all, for now redirect to the home page
app.get ("*", (req, res) => { res.redirect ("/"); });

// const port = process.env.port;
const port = 8080;

app.listen (port, () => console.log (`Server running on port ${port}`));