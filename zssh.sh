echo "eval '&(ssh-agent -s)'" =
eval "$(ssh-agent -s)"
echo "ssh-add ~/.ssh/id_ed25519_2026-09-05_Markiv_Aleksandr_GitHub" =
ssh-add ~/.ssh/id_ed25519_2026-09-05_Markiv_Aleksandr_GitHub
echo ""
echo SSH_AUTH_SOCK = $SSH_AUTH_SOCK
echo SSH_AGENT_PID = $SSH_AGENT_PID
echo ssh-add -l = 
ssh-add -l
echo ssh -T git@github.com = 
ssh -T git@github.com