import React, { Component } from 'react';

// react router
import { BrowserRouter as Router, Route, Switch }  from 'react-router-dom';

// Redux
import { Provider } from 'react-redux';
import myStore from './myStore';

import jwt_decode from 'jwt-decode';

// Layout components
import Navbar from './components/layout/Navbar';
import Footer from './components/layout/Footer';

// Main components
import Landing from './components/main/Landing';
import About from './components/main/About';
import Games from './components/main/Games';
import Contact from './components/main/Contact';

import Test from './components/test/Test';

// Auth components
import Register from './components/auth/Register';
import Login from './components/auth/Login';

import Profile from './components/profile/Profile';

import NotFound from './components/utils/NotFound';

import PrivateRoute from './components/common/PrivateRoute';

// my actions
import { setCurrentUser, logoutUser } from './actions/authActions';

// // misc
// import setAuthToken from './utils/setAuthToken';
// import { clearCurrentProfile } from './actions/profileActions';

// check for token
// if (localStorage.jwtToken) {
// 	// set auth token header auth
// 	setAuthToken (localStorage.jwtToken);

// 	// decode token and get user info
// 	let decoded = jwt_decode (localStorage.jwtToken);

// 	myStore.dispatch (setCurrentUser (decoded));

// 	// check for expired token
// 	let currentTime = Date.now () / 1000;
// 	if (decoded.exp < currentTime) {
// 		// logout the user
// 		myStore.dispatch (logoutUser ());

// 		myStore.dispatch (clearCurrentProfile ());

// 		// redirect to the landing page
// 		window.location.href = '/';
// 	}
// }

class App extends Component {

	render () {
		return (
		<Provider store= { myStore }>
			<Router>
				<div className="App">
					
					<Navbar />
					<Route exact path='/' component={ Landing } />
					<div className="container">
						{/* <Route exact path="/test" component={Test} /> */}

						{/* Main components */}
						<Route exact path="/about" component={ About } />
						<Route exact path="/contact" component={ Contact } />
						<Route exact path="/games" component={ Games } />

						{/* User */}
						<Route exact path="/register" component={ Register } />
						<Route exact path="/login" component={ Login } />
						<Route exact path="/profile/:handle" component={ Profile } />

						<Route exact path="/not-found" component={ NotFound } />
					</div>

					<Footer />
				</div>
			</Router>
		</Provider>
		);
	}

}

export default App;