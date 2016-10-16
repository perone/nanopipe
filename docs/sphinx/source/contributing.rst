.. _contributing:

Contributing
===============================================================================

If you want to contribute, first of all, take a look at our Github repository
to check if the feature you will work isn't already being developed. If not,
create an issue on Github to discuss what you're going to work on and if
it is a good idea. Make sure to describe what and how you want to implement
a feature or contribution.

Once you have discussed what you want to work on, then you will want to:

- Fork the official repository.
- Clone your fork::

    git clone git@github.com:<your-username>/nanopipe.git

- Make sure tests are passing for you::

    make && make test

- Create a topic branch::

    git checkout -b new-feature

- Add tests and code for your changes.
- Once you're done, make sure all tests still pass::

    make && make test

- Commit and push to your fork.
- Create an issue with a link to your patch.
- Sit back and enjoy.

There are other ways to help:

- Fix a bug or share your experience on issues
- Improve the documentation
- Help maintain or create new client libraries
- Improve this very website
