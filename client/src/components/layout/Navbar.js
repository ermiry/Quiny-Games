import React, { Component } from 'react';

import { Link } from 'react-router-dom';

// redux
import PropTypes from 'prop-types';
import { connect } from 'react-redux';

// my actions
import { logoutUser } from '../../actions/authActions';
import { clearCurrentProfile } from '../../actions/profileActions';

class Navbar extends Component {

    onLogOutClick (e) {

        e.preventDefault ();
        this.props.clearCurrentProfile ();
        this.props.logoutUser ();

    }

    render () {
        let { isAuthenticated } = this.props.auth;

        // TODO: display the profile image as the link
        /* TODO: change to an account drop down menu */
        const authLinks = (
            <ul className="navbar-nav ml-auto">
                <li className="nav-item">
                    <Link className="nav-link" to="/profile">Profile</Link>
                </li>
                <li className="nav-item">
                    <a href="/" onClick={ this.onLogOutClick.bind (this) } 
                    className="nav-link">
                    Logout</a>
                </li>
            </ul>
        );

        const guestLinks = (
            <ul className="navbar-nav ml-auto">
                <li className="nav-item">
                    <Link className="nav-link" to="/register">Sign Up</Link>
                </li>
                <li className="nav-item">
                    <Link className="nav-link" to="/login">Login</Link>
                </li>
            </ul>
        );

        return (
            <nav className="navbar navbar-expand-sm navbar-dark bg-dark mb-4">
                <div className="container">
                    <Link className="navbar-brand" to="/">Ermiry</Link>
                    <button className="navbar-toggler" type="button" data-toggle="collapse" data-target="#mobile-nav">
                        <span className="navbar-toggler-icon"></span>
                    </button>
                
                    <div className="collapse navbar-collapse" id="mobile-nav">
                        <ul className="navbar-nav mr-auto">
                            <li className="nav-item">
                                <Link className="nav-link" to="/projects">Projects</Link>
                            </li>
                            <li className="nav-item">
                                <Link className="nav-link" to="/games">Games</Link>
                            </li>
                            <li className="nav-item">
                                <Link className="nav-link" to="/contact">Contact</Link>
                            </li>
                        </ul>
                
                        { isAuthenticated ? authLinks : guestLinks }
                    </div>
                </div>
          </nav>
        )
    }

}

Navbar.propTypes = {
    logoutUser: PropTypes.func.isRequired,
    auth: PropTypes.object.isRequired
};
  
const mapStateToProps = state => ({
    auth: state.auth
});
  
export default connect (mapStateToProps, { logoutUser, clearCurrentProfile }) (
    Navbar
);