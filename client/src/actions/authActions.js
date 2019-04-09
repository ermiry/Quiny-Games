import axios from 'axios';

import setAuthToken from '../utils/setAuthToken';
import jwt_decode from 'jwt-decode';

import { SET_CURRENT_USER, GET_ERRORS } from './types';

// register a new user
export const registerUser = (userData, history) => dispatch => {

    // make the request to the server
    axios
        .post ('/api/users/register', userData)
        .then (res => history.push ('/login'))
        .catch (err =>
            dispatch ({
                type: GET_ERRORS,
                payload: err.response.data
            }));

}

export const loginUser = (userData) => dispatch => {

    axios
        .post ('/api/users/login', userData)
        .then (res => {
            let { token } = res.data;

            // set token to local storage
            localStorage.setItem ('jwtToken', token);

            // set token to auth header
            setAuthToken (token);

            // decode token to get user data
            let decoded = jwt_decode (token);

            // set current user
            dispatch (setCurrentUser (decoded));
        })
        .catch (err =>
            dispatch ({
                type: GET_ERRORS,
                payload: err.response.data
            }));

};

// set logged user
export const setCurrentUser = (decoded) => {

    return {
        type: SET_CURRENT_USER,
        payload: decoded
    }

}

// log user out
export const logoutUser = () => dispatch => {

    // remove token from localStorage
    localStorage.removeItem('jwtToken');
    
    // remove auth header for future requests
    setAuthToken (false);
    dispatch (setCurrentUser ({}));

};