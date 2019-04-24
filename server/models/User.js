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
    role: {
        type: String,
        required: true
    },
    location: {
        type: String,
        default: 'No location'
    },
    school: {
        type: String,
        default: 'No school'
    },
    avatar: {
        // FIXME: add place holder avatar img
        type: String,
        default: ' '
    },
    gamesPlayed: {
        type: Number,
        default: 0
    },
    wins: {
        type: Number,
        default: 0
    },
    friends: [
        {
            type: Schema.Types.ObjectId,
            ref: 'users'
        }
    ],
    token: {
        type: String,
        default: ''
    }
});

module.exports = User = mongoose.model ('users', UserSchema);