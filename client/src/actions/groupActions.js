import axios from 'axios';

import {
	GET_GROUP,
	GROUP_LOADING,
	GROUP_NOT_FOUND,
	CLEAR_CURRENT_GROUP,

	GET_ERRORS,
	CLEAR_ERRORS,
	SET_CURRENT_GROUP
} from './types';

// create group
export const createGroup = groupData => dispatch => {

	dispatch (clearErrors ());
	axios.post ('/api/groups', postData)
		.then (res => {
			dispatch ({
				type: CREATE_GROUP,
				payload: res.data
			})
		})
		.catch (err => {
			dispatch ({
				type: GET_ERRORS,
				payload: err.response.data
			})
		});

};

export const getGroupById = id => dispatc => {

	dispatch (setGroupLoading ());
	axios.get ('/api/groups/' + id)
		.then (res => 
			dispatch ({
				type: GET_GROUP,
				payload: res.data
			}))
		.catch (err => 
			dispatch ({
				type: GET_GROUP,
				payload: null
			}));

};

// FIXME: add update route!!

// delete a group
export const deleteGroup = id = dispatch => {

	if (window.confirm ('Are you sure? This can NOT be undone!')) {
		axios.delete ('/api/groups/' + id)
			.then (res => {
				dispatch ({
					type: SET_CURRENT_GROUP,
					payload: {}
				})
			})
			.catch (err => {
				dispatch ({
					type: GET_ERRORS,
					payload: err.response.data
				})
			});
	}

};

// Group loading
export const setGroupLoading = () => {
	return {
		type: GROUP_LOADING
	};
};
  
// Clear group
export const clearCurrentProfile = () => {
	return {
		type: CLEAR_CURRENT_GROUP
	};
};