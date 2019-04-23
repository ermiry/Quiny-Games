const express = require ('express');
const router = express.Router ();

const axios = require ('axios');
const request = require ('superagent');

const gamesurl = 'http://localhost:9001/';

// @route   GET api/games/test
// @desc    Test game routes
router.get ('/test', (req, res) => {

	console.log ("Games test\n");

	axios ({
		method: 'get',
		url: gamesurl + "?action=test",
		// headers: { Connection: 'keep-alive' }
	})
        .then (result => {
			// console.log (result);
			return res.status (200).json ({ msg: result.data });
		})
		.catch (err => {
			console.error (err);
			return res.status (400).json ({ msg: 'Failed test!'});
		});

});

/*** Ask ***/

// @route   GET api/games/ask/test
// @desc    Test that the ask game is running
router.get ('/ask/test', (req, res) => {

	axios.get (gamesurl + "?game=ask&action=test")
		.then (result => {
			console.log (result.data);
			res.send (result.data);
		})
		.catch (err => {
			console.error (err);
			return res.status (400).json ({ msg: 'Failed test!'});
		});

});

// TODO: to be part of a game, do we need to check for a user token and make our routes private?

// @route   POST api/games/ask/join
// @desc    Join an ask game lobby
router.post ('/ask/create', (req, res) => {

	// get the game creation parameters

	// make a request to the game to create the new lobby
	axios.get (gamesurl + "?game=ask&action=create_lobby")
		.then (result => {
			// TODO: return the new lobby id
			// FIXME: how can we keep updating this info?
			// console.log (result.data);
			// return res.status (200).json (res.data);
		})
		.catch (err => {
			console.error (err);
			return res.status (400).json ({ askError: 'Failed to create lobby.'});
		});

});

// @route   POST api/games/ask/join
// @desc    Join an ask game lobby
router.post ('/ask/join', (req, res) => {

	// get the session id of the game we want to join

	// request the game to join the game
	// FIXME: pass the lobby id
	axios.get (gamesurl + "?game=ask&action=join_lobby")
		.then (result => {
			// TODO: return the new lobby id & a list of the current players
			// console.log (result.data);
			// return res.status (200).json (res.data);
		})
		.catch (err => {
			console.error (err);
			return res.status (400).json ({ askError: 'Failed to join lobby.'});
		});

});

// @route   POST api/games/ask/leave
// @desc    Leave an ask game lobby
router.post ('/ask/leave', (req, res) => {

	// get the session id of the game we want to leave
	let token = req.query.token;

	// request the game to leave the game lobby
	axios.get (gamesurl + "?game=ask&action=start&token=" + token)
		.then (result => {
			// TODO: return a success code?
			// console.log (result.data);
			// return res.status (200).json (res.data);
		})
		.catch (err => {
			console.error (err);
			return res.status (400).json ({ askError: 'Failed to leave lobby.'});
		});

});

// request from alexa!!
// @route   GET api/games/ask/start
// @desc    Start ask game
router.get ('/ask/start', (req, res) => {

	// request the game to leave the game lobby
	// FIXME: pass the lobby id
	axios.get (gamesurl + "?game=ask&action=leave_lobby")
		.then (result => {
			// TODO: return a success code?
			// console.log (result.data);
			// return res.status (200).json (res.data);
		})
		.catch (err => {
			console.error (err);
			return res.status (400).json ({ askError: 'Failed to leave lobby.'});
		});

});

// request from alexa!!
// @route   GET api/games/ask/firstQuestion
// @desc    Get the first question for ask game
router.get ('/ask/firstQuestion', (req, res) => {

	// request the game to leave the game lobby
	// FIXME: pass the lobby id
	axios.get (gamesurl + "?game=ask&action=leave_lobby")
		.then (result => {
			// TODO: return a success code?
			// console.log (result.data);
			// return res.status (200).json (res.data);
		})
		.catch (err => {
			console.error (err);
			return res.status (400).json ({ askError: 'Failed to leave lobby.'});
		});

});

// request from alexa!!
// @route   POST api/games/ask/answer
// @desc    Recieves an answer fro a question
router.post ('/ask/answer', (req, res) => {

	// request the game to leave the game lobby
	// FIXME: pass the lobby id
	axios.get (gamesurl + "?game=ask&action=leave_lobby")
		.then (result => {
			// TODO: return a success code?
			// console.log (result.data);
			// return res.status (200).json (res.data);
		})
		.catch (err => {
			console.error (err);
			return res.status (400).json ({ askError: 'Failed to leave lobby.'});
		});

});

// request from alexa!!
// @route   GET api/games/ask/answer
// @desc    Gets the next question for the game
router.get ('/ask/getNextQuestion', (req, res) => {

	// request the game to leave the game lobby
	// FIXME: pass the lobby id
	axios.get (gamesurl + "?game=ask&action=leave_lobby")
		.then (result => {
			// TODO: return a success code?
			// console.log (result.data);
			// return res.status (200).json (res.data);
		})
		.catch (err => {
			console.error (err);
			return res.status (400).json ({ askError: 'Failed to leave lobby.'});
		});

});

// request from alexa!!
// @route   POST api/games/ask/answer
// @desc    Ends the game and retrieves scoreboard
router.get ('/ask/end', (req, res) => {

	// request the game to leave the game lobby
	// FIXME: pass the lobby id
	axios.get (gamesurl + "?game=ask&action=leave_lobby")
		.then (result => {
			// TODO: return a success code?
			// console.log (result.data);
			// return res.status (200).json (res.data);
		})
		.catch (err => {
			console.error (err);
			return res.status (400).json ({ askError: 'Failed to leave lobby.'});
		});

});


module.exports = router;