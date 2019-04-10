const mongoose = require ('mongoose');
const Schema = mongoose.Schema;

const UserSchema = new Schema ({

    name: {
        type: String,
        required: true
    },
    email: {
        type: String,
        required: true
    },
    username: {
        type: String,
        required: true
    },
    password: {
        type: String,
        required: true
    },
    location: {
        type: String
    },
    school: {
        type: String
    },
    role: {
        type: String,
        required: true
    },
    avatar: {
        type: String
    },
    gamesPlayed: {
        type: Number
    },
    wins: {
        type: Number
    },
    friends: [
        {
            type: Schema.Types.ObjectId,
            ref: 'users'
        }
    ]
});

module.exports = User = mongoose.model ('users', UserSchema);