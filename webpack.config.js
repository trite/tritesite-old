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
  // plugins: [
  //   new MiniCssExtractPlugin({
  //     filename: PROD ? "[name]-[contenthash].css" : "[name].css",
  //     chunkFilename: PROD ? "[name]-[contenthash].css" : "[name].css"
  //   })
  // ]
};