/**
 * @file Validated calculator module.
 * Provides basic arithmetic operations with strict input validation.
 */

/**
 * Validates that both inputs are finite numbers.
 * 
 * @param {*} a - The first input to validate.
 * @param {*} b - The second input to validate.
 * @throws {TypeError} If either input is not a primitive number.
 * @throws {TypeError} If either input is NaN, Infinity, or -Infinity.
 * @private
 */
function validateInputs(a, b) {
    // Check if both arguments are strict primitive numbers
    if (typeof a !== "number" || typeof b !== "number") {
        throw new TypeError("Both inputs must be numbers.");
    }

    // Reject non-finite values like NaN, Infinity, or -Infinity
    if (!Number.isFinite(a) || !Number.isFinite(b)) {
        throw new TypeError("Inputs must be finite numbers.");
    }
}

/**
 * Calculates the sum of two finite numbers.
 * 
 * @param {number} a - The first number.
 * @param {number} b - The second number.
 * @returns {number} The sum of a and b.
 * @throws {TypeError} If inputs are invalid or non-finite.
 */
function addition(a, b) {
    validateInputs(a, b);
    return a + b;
}

/**
 * Calculates the difference between two finite numbers.
 * 
 * @param {number} a - The minuend.
 * @param {number} b - The subtrahend.
 * @returns {number} The result of a minus b.
 * @throws {TypeError} If inputs are invalid or non-finite.
 */
function subtraction(a, b) {
    validateInputs(a, b);
    return a - b;
}

/**
 * Calculates the product of two finite numbers.
 * 
 * @param {number} a - The first factor.
 * @param {number} b - The second factor.
 * @returns {number} The product of a and b.
 * @throws {TypeError} If inputs are invalid or non-finite.
 */
function multiplication(a, b) {
    validateInputs(a, b);
    return a * b;
}

/**
 * Calculates the quotient of two finite numbers.
 * 
 * @param {number} a - The dividend.
 * @param {number} b - The divisor.
 * @returns {number} The result of a divided by b.
 * @throws {TypeError} If inputs are invalid or non-finite.
 * @throws {RangeError} If the divisor (b) is zero.
 */
function division(a, b) {
    validateInputs(a, b);

    // Prevent division by zero to avoid returning Infinity or -Infinity
    if (b === 0) {
        throw new RangeError("Cannot divide by zero.");
    }

    return a / b;
}

// Export the arithmetic API
module.exports = {
    addition,
    subtraction,
    multiplication,
    division
};
s