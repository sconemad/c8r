# c8r - A calculator

## Code structure

   - c8script     // Script parser/runner
   - c8eval       // Expression parser/evaluator
   - c8ctx        // Context

   - c8stmt       // Base statement
     - c8expr     // Expression
     - c8group    // Group
     - c8cond     // Conditional
     - c8loop     // Loop

   - c8obj        // Base object
     - c8error    // Error
     - c8bool     // Boolean
     - c8string   // String
     - c8list     // List
     - c8map      // Map
     - c8func     // Function/method
     - c8mpz      // GMP integer
     - c8mpfr     // MPFR float

   - c8buf        // Buffer
   - c8vec        // Vector
