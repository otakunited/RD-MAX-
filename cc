name: "⚡ EnigMano Win11 + CRD Cluster"

on:
  workflow_dispatch:

jobs:
  deploy-enigmano:
    runs-on: windows-latest
    strategy:
      matrix:
        instance: [1,2,3,4,5,6,7,8,9,10,
                   11,12,13,14,15,16,17,18,19,20]
      max-parallel: 20

    steps:
      - name: 📌 Deployment Parameters
        shell: pwsh
        run: |
          Write-Host "==============================================="
          Write-Host "🔹 EnigMano Instance : ${{ matrix.instance }}"
          Write-Host "📦 Repository        : $env:GITHUB_REPOSITORY"
          Write-Host "==============================================="

      - name: 📥 Install Chrome Remote Desktop
        shell: pwsh
        run: |
          $crdInstaller = "$env:TEMP\crdhost.msi"
          Invoke-WebRequest "https://dl.google.com/edgedl/chrome-remote-desktop/chromeremotedesktophost.msi" -OutFile $crdInstaller
          Start-Process msiexec.exe -ArgumentList "/i `"$crdInstaller`" /quiet /norestart" -Wait
          Remove-Item $crdInstaller -Force
          Write-Host "✅ Chrome Remote Desktop installed on Instance ${{ matrix.instance }}"

      - name: ⚔️ Start Chrome Remote Desktop Host
        shell: pwsh
        run: |
          $exe = "${Env:ProgramFiles(x86)}\Google\Chrome Remote Desktop\CurrentVersion\remoting_start_host.exe"

          if (-not (Test-Path $exe)) {
            Write-Error "❌ CRD executable not found at $exe"
            exit 1
          }

          & $exe `
            --code="4/0AVGzR1DUqm1_me-lo6y1JsfO3ESmv4emHSmQdHscadAHLItSWt2eXMgDWOzXfc3osGTxiA" `
            --redirect-url="https://remotedesktop.google.com/_/oauthredirect" `
            --name="EnigMano-${{ matrix.instance }}" `
            --pin=123456

      - name: 🕒 Keep Alive 6h Timer
        shell: pwsh
        run: |
          $total = 21600
          for ($s = $total; $s -ge 0; $s--) {
            if ($s % 300 -eq 0) {  # Affiche toutes les 5 min
              Write-Host "Instance ${{ matrix.instance }} → $s secondes restantes..."
            }
            Start-Sleep -Seconds 1
          }
          Write-Host "⏰ 6h écoulées pour l'Instance ${{ matrix.instance }}"

      - name: 💠 Final Status
        if: always()
        shell: pwsh
        run: |
          Write-Host "✅ EnigMano Instance ${{ matrix.instance }} deployed with Chrome Remote Desktop."
          Write-Host "🔋 Powered by: CRD + GitHub Actions Cluster"
