import { describe, it } from "mocha";
import { expect } from "chai";
import request from "supertest";
import app from "../src/app.js";

// Group tests for the GET /api/convert endpoint.
describe("GET /api/convert:", () => {
  // Verifies that a valid hex color returns the expected RGB values.
  it("returns RGB values for a valid hex color", async () => {
    const response = await request(app)
      .get("/api/convert")
      .query({ hex: "#FF5733" });

    expect(response.status).to.equal(200);
    expect(response.body).to.deep.equal({
      hex: "#FF5733",
      rgb: {
        r: 255,
        g: 87,
        b: 51,
      },
    });
  });

  // Ensures three-digit shorthand colors are supported.
  it("supports shorthand hex values", async () => {
    const response = await request(app)
      .get("/api/convert")
      .query({ hex: "F53" });

    expect(response.status).to.equal(200);
    expect(response.body.rgb).to.deep.equal({
      r: 255,
      g: 85,
      b: 51,
    });
  });

  // Checks that invalid hex colors return a 400 status.
  it("returns 400 for an invalid hex color", async () => {
    const response = await request(app)
      .get("/api/convert")
      .query({ hex: "XYZ" });

    expect(response.status).to.equal(400);
    expect(response.body).to.deep.equal({
      error: "Invalid hex color",
    });
  });

  // Ensures a missing hex parameter returns a 400 status.
  it("returns 400 when hex is missing", async () => {
    const response = await request(app)
      .get("/api/convert");

    expect(response.status).to.equal(400);
    expect(response.body).to.deep.equal({
      error: 'Query parameter "hex" is required',
    });
  });
});