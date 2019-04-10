const express = require ('express');
const router = express.Router ();

const bcrypt = require ('bcryptjs');
const jwt = require ('jsonwebtoken');
const keys = require ('../../../config/keys');
const passport = require ('passport');

// load user model
const User = require ('../../../models/User');

// load input Validation
const validateRegisterInput = require ('../../validation/register');
const validateLoginInput = require ('../../validation/register');

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
                // FIXME: check the new parameters
                let newUser = new User ({
                    name: req.body.name,
                    email: req.body.email,
                    username: req.body.username,
                    password: req.body.password
                });

                // TODO: generate the hash from the client?
                // hash the password
                bcrypt.genSalt (10, (err, salt) => {
                    bcrypt.hash (newUser.password, salt, (err, hash) => {
                        if (err) throw err;
                        newUser.password = hash;
                        newUser.save ()
                            .then (user => res.json (user))
                            .catch (err => console.error (err));
                    });
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

            // FIXME: work with the new parameters
            // check for password
            bcrypt.compare (password, user.password).then (isMatch => {
                if (isMatch) {
                    // retrive user data
                    // TODO: get ALL of our data to display in profile
                    let payload = { id: user.id, name: user.name, username: user.username, 
                        location: user.location, 
                        memberSince: user.memberSince, lastTime: user.lastTime };

                    // generate token
                    jwt.sign (payload, keys.secretOrKey, { expiresIn: 3600 }, 
                        (err, token) => {
                            res.json ({
                                success: true,
                                token: 'Bearer ' + token
                            });
                    });
                }

                else {
                    errors.password = 'Password is incorrect.';
                    return res.status (400).json (errors); 
                }
            });
            
        });

});

module.exports = router;