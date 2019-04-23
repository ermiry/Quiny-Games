const express = require ('express');
// const mongoose = require ('mongoose');
const bodyParser = require ('body-parser');
// const passport = require ('passport');

const app = express ();

// Body parser middleware
app.use (bodyParser.urlencoded ({ extended: false }));
app.use (bodyParser.json ());

/** Routes **/
app.set('view engine','ejs');

app.use(express.static ("views"));
const quiny = require ('./routes/api/quiny');
app.use ('/api/quiny', quiny);

// catch all, for now redirect to the home page
// app.get ("*", (req, res) => { res.redirect ("/"); });
const port = process.env.PORT || 10000;

app.listen (port, () => console.log (`Server running on port ${port}`));
