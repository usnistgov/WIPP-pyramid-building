# TODOS


## Running container with unprivileged user
The container is run with the root user.
This may be a security issue.
We should rather create a unprivileged user, add it to a user group.
Then set UID/GUID of the host directory to the UID/GID of this user.
We would then be able to mount this directory and run the algorithm as intended.

## Trim down version by just copying the compiled sources.

--------

## Add glog dependency