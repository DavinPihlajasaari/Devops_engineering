/**
 * Validates that both inputs are numbers and finite values.
 *
 * @param {number} a - The first value to validate.
 * @param {number} b - The second value to validate.
 * @throws {TypeError} If either input is not a number.
 * @throws {TypeError} If either input is NaN, Infinity, or -Infinity.
 */
function validateInputs(a, b) {
    // Ensure both values have the JavaScript "number" type.
    if (typeof a !== "number" || typeof b !== "number") {
        throw new TypeError("Both inputs must be numbers.");
    }

    // Ensure neither value is NaN or positive/negative Infinity.
    if (!Number.isFinite(a) || !Number.isFinite(b)) {
        throw new TypeError("Inputs must be finite numbers.");
    }
}

/**
 * Adds two numbers together.
 *
 * @param {number} a - The first number.
 * @param {number} b - The second number.
 * @returns {number} The sum of a and b.
 * @throws {TypeError} If either input is not a finite number.
 */
function addition(a, b) {
    validateInputs(a, b);
    return a + b;
}

/**
 * Subtracts the second number from the first number.
 *
 * @param {number} a - The number to subtract from.
 * @param {number} b - The number to subtract.
 * @returns {number} The difference between a and b.
 * @throws {TypeError} If either input is not a finite number.
 */
function subtraction(a, b) {
    validateInputs(a, b);
    return a - b;
}

/**
 * Multiplies two numbers together.
 *
 * @param {number} a - The first number.
 * @param {number} b - The second number.
 * @returns {number} The product of a and b.
 * @throws {TypeError} If either input is not a finite number.
 */
function multiplication(a, b) {
    validateInputs(a, b);
    return a * b;
}

/**
 * Divides the first number by the second number.
 *
 * @param {number} a - The dividend.
 * @param {number} b - The divisor.
 * @returns {number} The quotient of a divided by b.
 * @throws {TypeError} If either input is not a finite number.
 * @throws {RangeError} If b is zero.
 */
function division(a, b) {
    validateInputs(a, b);

    // JavaScript normally returns Infinity when dividing by zero.
    // We explicitly reject division by zero instead.
    if (b === 0) {
        throw new RangeError("Cannot divide by zero.");
    }

    return a / b;
}

/**
 * Exports the calculator functions so they can be used
 * by other files in a Node.js application.
 */
module.exports = {
    addition,
    subtraction,
    multiplication,
    division
};