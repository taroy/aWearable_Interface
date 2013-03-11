/**
 * Main RANGE constructor
 * Process options
 */
 
/**
 * Tell the board to set it up
 * @param object options
 */
var Range = function (options) {
  if (!options || !options.board) throw new Error('Must supply required range options');
  this.board = options.board;
  this.range = options.range || 100;
  console.log("duinoRange");
}

module.exports = Range;