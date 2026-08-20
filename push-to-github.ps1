# Push NexConnect Setup to GitHub
# This script helps you push your code to GitHub repository

Write-Host "======================================" -ForegroundColor Cyan
Write-Host "Push NexConnect Setup to GitHub" -ForegroundColor Cyan
Write-Host "======================================" -ForegroundColor Cyan
Write-Host ""

# Check if gh CLI is available
$ghPath = Get-Command gh -ErrorAction SilentlyContinue
if ($ghPath) {
    Write-Host "[INFO] GitHub CLI detected, checking authentication..." -ForegroundColor Green
    
    $authStatus = gh auth status 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Host "[OK] Already authenticated with GitHub CLI" -ForegroundColor Green
        Write-Host ""
        Write-Host "Pushing to GitHub using gh CLI..." -ForegroundColor Yellow
        
        git push -u origin main
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host ""
            Write-Host "[SUCCESS] Code pushed to GitHub!" -ForegroundColor Green
            Write-Host "Repository: https://github.com/MouseTi/NexConnectSetup" -ForegroundColor Cyan
        } else {
            Write-Host ""
            Write-Host "[ERROR] Push failed. Please check your permissions." -ForegroundColor Red
        }
        exit
    } else {
        Write-Host "[INFO] Not authenticated. Logging in..." -ForegroundColor Yellow
        gh auth login
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host ""
            Write-Host "Pushing to GitHub..." -ForegroundColor Yellow
            git push -u origin main
            
            if ($LASTEXITCODE -eq 0) {
                Write-Host ""
                Write-Host "[SUCCESS] Code pushed to GitHub!" -ForegroundColor Green
                Write-Host "Repository: https://github.com/MouseTi/NexConnectSetup" -ForegroundColor Cyan
            }
        }
        exit
    }
}

Write-Host "[WARNING] GitHub CLI not found." -ForegroundColor Yellow
Write-Host ""
Write-Host "Please choose authentication method:" -ForegroundColor Cyan
Write-Host "1. Setup SSH key (recommended)" -ForegroundColor White
Write-Host "2. Use GitHub Personal Access Token" -ForegroundColor White
Write-Host "3. Install GitHub CLI (easiest)" -ForegroundColor Green
Write-Host ""
$choice = Read-Host "Enter your choice (1-3)"

switch ($choice) {
    "1" {
        Write-Host ""
        Write-Host "=== SSH Key Setup ===" -ForegroundColor Cyan
        Write-Host "1. Generate SSH key: ssh-keygen -t ed25519 -C 'your_email@example.com'" -ForegroundColor White
        Write-Host "2. Copy public key: cat ~/.ssh/id_ed25519.pub" -ForegroundColor White
        Write-Host "3. Add to GitHub: https://github.com/settings/ssh/new" -ForegroundColor White
        Write-Host "4. Run: git push -u origin main" -ForegroundColor White
    }
    "2" {
        Write-Host ""
        Write-Host "=== Personal Access Token ===" -ForegroundColor Cyan
        Write-Host "1. Create token: https://github.com/settings/tokens/new" -ForegroundColor White
        Write-Host "   - Select 'repo' scope" -ForegroundColor Gray
        Write-Host "2. Change remote to HTTPS:" -ForegroundColor White
        Write-Host "   git remote set-url origin https://github.com/MouseTi/NexConnectSetup.git" -ForegroundColor Gray
        Write-Host "3. Push (will ask for token):" -ForegroundColor White
        Write-Host "   git push -u origin main" -ForegroundColor Gray
    }
    "3" {
        Write-Host ""
        Write-Host "=== Install GitHub CLI ===" -ForegroundColor Cyan
        Write-Host "1. Download: https://cli.github.com/" -ForegroundColor White
        Write-Host "   OR using winget: winget install --id GitHub.cli" -ForegroundColor Gray
        Write-Host "2. After install, run this script again" -ForegroundColor White
        
        $install = Read-Host "`nInstall via winget now? (y/n)"
        if ($install -eq "y") {
            winget install --id GitHub.cli
            Write-Host ""
            Write-Host "Please restart your terminal and run this script again." -ForegroundColor Green
        }
    }
    default {
        Write-Host "[ERROR] Invalid choice" -ForegroundColor Red
    }
}

Write-Host ""
pause
