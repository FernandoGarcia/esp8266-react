const { WebpackManifestPlugin } = require('webpack-manifest-plugin');
const MiniCssExtractPlugin = require('mini-css-extract-plugin');
const ProgmemGenerator = require('./progmem-generator.js');
const TerserPlugin = require('terser-webpack-plugin');

module.exports = function override(config, env) {
  if (env === "production") {
    // rename the ouput file, we need it's path to be short, for embedded FS
    config.output.filename = 'js/[id].[chunkhash:4].js';
    config.output.chunkFilename = 'js/[id].[chunkhash:4].js';

    // take out the manifest plugin
    config.plugins = config.plugins.filter((plugin) => !(plugin instanceof WebpackManifestPlugin));

    // shorten css filenames
    const miniCssExtractPlugin = config.plugins.find((plugin) => plugin instanceof MiniCssExtractPlugin);
    miniCssExtractPlugin.options.filename = "css/[id].[contenthash:4].css";
    miniCssExtractPlugin.options.chunkFilename = "css/[id].[contenthash:4].c.css";

    // don't emit license file
    const terserPlugin = config.optimization.minimizer.find((plugin) => plugin instanceof TerserPlugin);
    terserPlugin.options.extractComments = false;

    // build progmem data files
    config.plugins.push(new ProgmemGenerator({ outputPath: "../src/WWWData.h", bytesPerLine: 20 }));
  }
  return config;
};
