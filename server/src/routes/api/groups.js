const express = require ('express');
const router = express.Router ();
const passport = require('passport');

// load user model
const Group = require ('../../../models/Group');

// @route   GET api/groups/test
// @desc    Tests groups route
router.get ('/test', (req, res) => res.json ({ msg: 'Groups Works' }));

// @route   POST api/groups/create
// @desc    a user creates a new group
router.post ('/create', passport.authenticate ('jwt', { session: false }), (req, res) => {

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
        leader: req.user.id
    });

    newGroup.save ().then (group => res.json (group));

});

// @route   GET api/groups/create
// @desc    // gets a group
router.get ('/:id', (req, res) => {

    Post.findById (req.params.id)
        .then (group => {
            if (group) res.json (group);
            else res.status (404).json ({ groupnotfound: 'No group found with that id' });
        })
        .catch (err => {
            res.status (404).json ({ groupnotfound: 'No group found with that id' });
        });

});

// @route   POST api/groups/create
// @desc    updates a group
router.post ('/:id', passport.authenticate ('jwt', { session: false }), (req, res) => {

    // TODO: only adds a new user to the members

});

// @route   POST api/groups/:id
// @desc    the leader of the group deletes it by id
router.delete ('/:id', passport.authenticate ('jwt', { session: false }), (req, res) => {

    Post.findById (req.params.id)
        .then (group => {
            // check for the leader
            if (group.leader.toString () !== req.user.id) {
                return res
                    .status (401)
                    .json ({ notauthorized: 'User not authorized' });
            }

            // delete the post
            group.remove ().then (() => res.json ({ success: true }));
        })
        .catch (err => res.status (404).json ({ groupnotfound: 'Group not found'}));

});

module.exports = router;