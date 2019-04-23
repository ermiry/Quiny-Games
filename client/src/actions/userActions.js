import axios from 'axios';

import {
	GET_PROFILE,
	GET_PROFILES,
	PROFILE_LOADING,
	CLEAR_CURRENT_PROFILE,
	GET_ERRORS,
	SET_CURRENT_USER
} from './types';

// TODO: do we need this?
// Get current profile
export const getCurrentProfile = () => dispatch => {

  dispatch (setProfileLoading());
  axios
    .get ('/api/user')
    .then(res =>
      dispatch({
        type: GET_PROFILE,
        payload: res.data
      })
    )
    .catch(err =>
      dispatch({
        type: GET_PROFILE,
        payload: {}
      })
    );
};

// Get profile by handle
export const getProfileByHandle = handle => dispatch => {

  dispatch (setProfileLoading());
  axios
    .get (`/api/user/${handle}`)
    .then(res =>
      dispatch({
        type: GET_PROFILE,
        payload: res.data
      })
    )
    .catch(err =>
      dispatch({
        type: GET_PROFILE,
        payload: null
      })
	);
	
};

// FIXME: how do we get the correct user?
// Delete account & profile
export const deleteAccount = () => dispatch => {

  if (window.confirm ('Are you sure? This can NOT be undone!')) {
    axios
      .delete ('/api/user')
      .then (res =>
        dispatch({
          type: SET_CURRENT_USER,
          payload: {}
        })
      )
      .catch(err =>
        dispatch({
          type: GET_ERRORS,
          payload: err.response.data
        })
      );
  }
};

// Profile loading
export const setProfileLoading = () => {
  return {
    type: PROFILE_LOADING
  };
};

// Clear profile
export const clearCurrentProfile = () => {
  return {
    type: CLEAR_CURRENT_PROFILE
  };
};