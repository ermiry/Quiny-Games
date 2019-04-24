const express = require ('express');
const router = express.Router ();

const bcrypt = require ('bcryptjs');
const jwt = require ('jsonwebtoken');
// const keys = require ('../../../config/keys');
// const passport = require ('passport');

// load user model
const User = require ('../../../models/User');

// load input Validation
const validateRegisterInput = require ('../../validation/register');
const validateLoginInput = require ('../../validation/register');

const gamesurl = 'http://localhost:9001/';

// @route   GET api/users/test
// @desc    Tests users route
router.get ('/test', (req, res) => res.json ({ msg: 'Users Works' }));

// @route   POST api/users/register
// @desc    Register user
router.post ('/register', (req, res) => {

    // check input validation
    let { errors, isValid } = validateRegisterInput (req.body);
    if (!isValid) return res.status (400).json (errors);

    // search for a user registerd with that email
    User.findOne ({ email: req.body.email })
        .then (user => {
            if (user) {
                errors.email = 'The email was already registered.';
                return res.status (400).json (errors);
            }
        });

    // search for a user with that username
    User.findOne ({ username: req.body.username })
        .then (user => {
            if (user) {
                errors.username = 'Username already exists.';
                return res.status (400).json (errors);
            }

            // register the new user
            else {
                let newUser = new User ({
                    name: req.body.name,
                    email: req.body.email,
                    username: req.body.username,
                    password: req.body.password,
                    role: req.body.role
                });

                // FIXME: generate the hash from the client?
                // hash the password
                /* bcrypt.genSalt (10, (err, salt) => {
                    bcrypt.hash (newUser.password, salt, (err, hash) => {
                        if (err) throw err;
                        newUser.password = hash;
                        newUser.save ()
                            .then (user => res.json (user))
                            .catch (err => console.error (err));
                    });
                }); */

                // FIXME: for now, store the password in plain text
                // add the new user to the db
                newUser.save ().then (user => {
                    // make a request to the cerver to login
                    axios.get (gamesurl + "?action=login&email=" + user.email + "&password=" + user.password)
                        .then (result => {
                            // return the user as json
                            res.status (200).json (user);
                        })
                        .catch (err => {
                            console.error (err);
                            res.status (400).send (err.data);
                        });
                })
                .catch (err => {
                    console.error (err);
                    res.status (500).json ({ error: 'Internal server error!' });
                });
            }
        });

});

// @route   GET api/users/login
// @desc    Login User / Returning JWT Token
router.post ('/login', (req, res) => {

    // check input validation
    let { errors, isValid } = validateLoginInput (req.body);
    if (!isValid) return res.status (400).json (errors);

    let email = req.body.email;
    let password = req.body.password;
    module.exports = router;
    // find user by email
    User.findOne ({ email })
        .then (user => {
            if (!user) {
                errors.email = 'User not found.';
                return res.status (404).json (errors);
            } 

            // login in the cerver with credentials
            // make a request to the cerver to login
            axios.get (gamesurl + "?action=login&email=" + user.email + "&password=" + user.password)
            .then (result => {
                // TODO: check for errors here!

                // TODO: add support for friends!!
                let payload = { id: user.id, name: user.name, username: user.username, email: user.email,
                    location: user.location, school: user.school, role: user.role,
                    gamesPlayed: user.gamesPlayed, wins: user.wins };

                // generate token
                jwt.sign (payload, keys.secretOrKey, { expiresIn: 3600 }, 
                    (err, token) => {
                        res.json ({
                            success: true,
                            token: 'Bearer ' + token
                        });
                });
            })
            .catch (err => {
                console.error (err);
                res.status (400).send (err.data);
            });
        });

});

module.exports = router;