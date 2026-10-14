// math.js

/**
 * Adds two numbers together.
 *
 * @param {number} a - The first number to add.
 * @param {number} b - The second number to add.
 * @returns {number} The sum of `a` and `b`.
 * @throws {Error} If either argument is not a number.
 */
function add(a, b) {
  if (typeof a !== "number" || typeof b !== "number") {
    throw new Error("Inputs must be numbers");
  }

  return a + b;
}

/**
 * Checks if a number is even.
 *
 * @param {number} n - The number to check.
 * @returns {boolean} `true` if the number is even, otherwise `false`.
 * @throws {Error} If the argument is not a number.
 */
function isEven(n) {
  if (typeof n !== "number") {
    throw new Error("Input must be a number");
  }
  return n % 2 === 0;
}

/**
 * Math utility functions.
 *
 * @module math
 * @property {function(number, number): number} add - Adds two numbers.
 * @property {function(number): boolean} isEven - Checks if a number is even.
 */
module.exports = { add, isEven };

