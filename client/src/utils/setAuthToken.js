import axios from 'axios';

const setAuthToken = token => {

	// Apply to every request
	if (token) axios.defaults.headers.common['Authorization'] = token;
	else delete axios.defaults.headers.common['Authorization'];
	
};

export default setAuthToken;