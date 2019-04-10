const express = require ('express');
const router = express.Router ();

const axios = require ('axios');
const request = require ('superagent');

const gamesurl = 'http://localhost:7000/';

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

module.exports = router;