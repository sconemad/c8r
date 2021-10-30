# c8r - A calculator

## Code structure

   - __c8script__     Script parser/runner
   - __c8eval__       Expression parser/evaluator
   - __c8ctx__        Context

   - __c8stmt__       Base statement
     - __c8expr__     Expression
     - __c8group__    Group
     - __c8cond__     Conditional (if)
     - __c8loop__     Loop (while, for)
     - __c8flow__     Flow (return, last, next)
     - __c8decl__     Variable declaration
     - __c8subdef__   Subroutine definition

   - __c8obj__        Base object
     - __c8error__    Error
     - __c8bool__     Boolean
     - __c8string__   String
     - __c8list__     List
     - __c8map__      Map
     - __c8func__     Function/method
     - __c8sub__      Subroutine
     - __c8num__      Numeric
     - __c8mpz__      GMP integer
     - __c8mpfr__     MPFR real number
     - __c8mpc__      MPC complex number

   - __c8buf__        Buffer
   - __c8vec__        Vector

   - __c8debug__      Debug logger
