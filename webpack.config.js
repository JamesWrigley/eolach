module.exports = {
    entry: './frontend/cognito.js',
    output: {
        path: __dirname + '/frontend',
        filename: 'eolach.js'
    },
    module: {
        rules: [
            {
                type: 'javascript/auto',
                test: /\.json$/,
                loader: 'json-loader'
            }
        ]
    }
};
