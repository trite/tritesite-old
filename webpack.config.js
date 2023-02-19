module.exports = {
  entry: "./_build/default/src/Blog.bs.js",
  output: {
    publicPath: "/",
  },
  devServer: {
    historyApiFallback: {
      index: "index.html"
    }
  },
};