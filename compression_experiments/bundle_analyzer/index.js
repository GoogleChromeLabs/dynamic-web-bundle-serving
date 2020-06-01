/**
 * @see https://github.com/azukaru/bundle-inspector/blob/master/src/lib/ASTanalyzer.js
 */

import acornLoose from "acorn-loose";
import get from "dlv";
import { predictModuleName } from "./predictions.js";

/** Dot-notated paths to look for within a bundle */
const bundleFunctionPaths = [
  "body.0.expression.right.arguments.0.elements",
  "body.0.expression.argument.arguments.0.elements",
  "body.0.expression.arguments.0.properties",
  "body.0.expression.arguments.0.elements.1.elements",
  "body.0.expression.arguments.0.elements.1.properties",
  "body.0.expression.right.arguments.1.body.body.0.declarations.0.init.arguments.1.elements",
  "body.2.expression.arguments.0.elements.1.properties"
].map(p => p.split("."));

/**
 * Tries several different path signatures to find the list of functions contained
 * within an AST representation of a webpack bundle
 *
 * @param {Object} syntaxTree - An AST representation of a webpack bundle
 */
function getFunctionNodes(syntaxTree) {
  return bundleFunctionPaths.reduce(
    (acc, path) => acc || get(syntaxTree, path),
    null
  );
}

/**
 * Analyzes a webpack bundle in string form and returns useable stats and a list of functions
 *
 * @param {string} bundle - A webpack bundle
 * @return {object} A parsed bundle, with properties for bundle-wide stats and a list of functions
 */
export function analyze(bundle) {
  const ast = acornLoose.parse(bundle);
  const functionNodes = getFunctionNodes(ast).filter(Boolean);
  const functions = functionNodes.map((node, index) => {
    const code = bundle.substring(node.start, node.end);
    const name = predictModuleName(code);
    const id = index + 1;
    
    return { id, name, code };
  });
  return functions;
}

