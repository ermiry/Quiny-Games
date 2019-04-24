const express = require ('express');
const router = express.Router ();
const passport = require('passport');

// load user model
const Group = require ('../../../models/Group');

// @route   GET api/groups/test
// @desc    Tests groups route
router.get ('/test', (req, res) => res.json ({ msg: 'Groups Works' }));

router.post ( '/', passport.authenticate('jwt', { session: false }), (req, res) => {

      const { errors, isValid } = validatePostInput(req.body);
  
      // Check Validation
      if (!isValid) {
        // If any errors, send 400 with errors object
        return res.status(400).json(errors);
      }
  
      const newPost = new Post({
        text: req.body.text,
        name: req.body.name,
        avatar: req.body.avatar,
        user: req.user.id
      });
  
      newPost.save().then(post => res.json(post));
    }
  );

// @route   POST api/groups/create
// @desc    create a new group
router.post ( '/create', passport.authenticate ('jwt', { session: false }), (req, res) => {

    let errors = {};

    // TODO: this might change in the future
    Group.findOne ({ name: req.body.name })
        .then (group => {
            if (group) {
                errors.group = 'There is already a group with that name.';
                return res.status (400).json (errors);
            }
        });

    // create the new group
    let newGroup = new Group ({
        name: req.body.name,
        leader: req.body.email,
        username: req.body.username,
        password: req.body.password,
        role: req.body.role
    });


});

// @route   GET api/groups/create
// @desc    // gets a group
router.get ('/:id', (req, res) => {

});

// @route   POST api/groups/create
// @desc    updates a group
router.post ('/:id', (req, res) => {

});

// @route   POST api/groups/:id
// @desc    deletes a group by id
router.delete ('/:id', (req, res) => {

});

module.exports = router;