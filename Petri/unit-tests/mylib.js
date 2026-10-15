function validateInputs(a, b) {
    if (typeof a !== "number" || typeof b !== "number") {
        throw new TypeError("Both inputs must be numbers.");
    }

    if (!Number.isFinite(a) || !Number.isFinite(b)) {
        throw new TypeError("Inputs must be finite numbers.");
    }
}

function addition(a, b) {
    validateInputs(a, b);
    return a + b;
}

function subtraction(a, b) {
    validateInputs(a, b);
    return a - b;
}

function multiplication(a, b) {
    validateInputs(a, b);
    return a * b;
}

function division(a, b) {
    validateInputs(a, b);

    if (b === 0) {
        throw new RangeError("Cannot divide by zero.");
    }

    return a / b;
}

module.exports = {
    addition,
    subtraction,
    multiplication,
    division
};
