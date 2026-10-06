const { expect } = require("chai");
const mylib = require("../mylib");

describe("mylib arithmetic functions", function () {

    before(function () {
        console.log("Starting mylib tests...");
    });

    after(function () {
        console.log("Finished mylib tests.");
    });

    it("should add two numbers", function () {
        expect(mylib.addition(10, 5)).to.equal(15);
    });

    it("should subtract two numbers", function () {
        expect(mylib.subtraction(10, 5)).to.equal(5);
    });

    it("should multiply two numbers", function () {
        expect(mylib.multiplication(10, 5)).to.equal(50);
    });

    it("should divide two numbers", function () {
        expect(mylib.division(10, 5)).to.equal(2);
    });

    it("should throw an error when dividing sby zero", function () {
        expect(() => mylib.division(10, 0))
            .to.throw(RangeError, "Cannot divide by zero.");
    });
    
    it("should throw an error when inputs are not numbers", function () {
    expect(() => mylib.addition(10, "a"))
        .to.throw(TypeError, "Both inputs must be numbers.");
    });
});
