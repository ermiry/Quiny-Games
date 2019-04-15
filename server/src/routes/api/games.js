const express = require ('express');
const router = express.Router ();

const axios = require ('axios');
const request = require ('superagent');

const gamesurl = 'http://localhost:7000/';

// @route   Get api/games/test
// @desc    Test game routes
router.get ('/test', (req, res) => {

	// request.get (gamesurl)
	// 	.then (result => {
	// 		console.log (result);
	// 		return res.status (200).json (result.body);
	// 	})
	// 	.catch (err => {
	// 		console.error (err.message);
	// 		return res.status (400).json ({ msg: 'Failed test!'});
	// 	});

	console.log ("Games test\n");

	axios.get (gamesurl)
        .then (result => {
			console.log (result.data);
			return res.status (200).json (res.data);
		})
		.catch (err => {
			console.error (err);
			return res.status (400).json ({ msg: 'Failed test!'});
		});

});

/*** Ask ***/

// @route   Get api/games/ask/test
// @desc    Test that the ask game is running
router.get ('/ask/test', (req, res) => {

	axios.get (gamesurl + "games?name=ask&action=test")
		.then (result => {
			console.log (result.data);
			return res.status (200).json (res.data);
		})
		.catch (err => {
			console.error (err);
			return res.status (400).json ({ msg: 'Failed test!'});
		});

});

// TODO: to be part of a game, do we need to check for a user token and make our routes private?

module.exports = router;