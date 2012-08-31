# ZNC-Modules

Efreak's ZNC Modules

I wrote the original versions of these modules\*, and I maintain them\*\* (sorta).

\*This is to the best of my knowledge; I've downloaded modules all over the place, and if you think one of my modules is based on another, let me know and I'll fix this.

\*\* Feel free to fork or ask for contributor access. I've pretty much lost interest.

## My Projects

### Whois
This global module catches whois commands sent to the server. If the username begins with a ? (i.e. ?efreak), it shows varying amounts of information, depending on the settings and whether or not you are an admin. This module by default sends numerics, to be compatible with existing scripts that use whois.

### Broadcast
This global module provides the same functionality as the broadcast command (it sends a notice to all the users), but with a standard prefix.

### DNS
This global module resolves hostnames on the server.

### QuitAway
This user module sets your away message from your quit message.

### KillNotice
This global module sends a notice to all admins whenever a user is killed off a server.

## Version Numbers
My version numbers reflect the version of ZNC that version of code was designed to work with. If the version is 0.01, this is because the module hasn't been modified since I changed the versioning scheme, and I don't know which version it was written for. Feel free to update the 0.01 version number if you verify it working on a current version of ZNC.

## Updating My Code
If you give me your login info on github, I'll add you as a contributor (if I can figure out how). This'll allow you to update my repo, and it'll put you down as the author of those changes. The basics of GIT are pretty simple to use, once you get it set up (the basics are all I know--git commit & push). Reply here & contact me on IRC.
