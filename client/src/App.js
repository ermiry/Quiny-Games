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
import Landing from './components/layout/Landing';

import Test from './components/test/Test';

// Auth components
import Register from './components/auth/Register';
import Login from './components/auth/Login';

// FIXME:
// import Profile from './components/profile/Profile';

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
						<Route exact path="/test" component={Test} />

						{/* User */}
						<Route exact path="/register" component={ Register } />
						<Route exact path="/login" component={ Login } />

						{/* <Switch>
							<PrivateRoute exact path="/profile" component={ Profile } />
						</Switch> */}
					</div>

					<Footer />
				</div>
			</Router>
		</Provider>
		);
	}

}

export default App;