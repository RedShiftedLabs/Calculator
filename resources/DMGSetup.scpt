on run argv
    if length of argv is less than 1 then
        display dialog "Usage: DMGSetup.scpt <Calculator>"
        return
    end if
    
    set volumeName to item 1 of argv
    
    try
        tell application "Finder"
            tell disk volumeName
                open
                
                -- Set view options
                set current view of container window to icon view
                set toolbar visible of container window to false
                set statusbar visible of container window to false
                set the bounds of container window to {400, 100, 900, 400}
                
                set theViewOptions to the icon view options of container window
                set arrangement of theViewOptions to not arranged
                set icon size of theViewOptions to 72
                set background picture of theViewOptions to file ".background:dmg-background.png"
                
                -- Position items
                set position of item "Calculator.app" of container window to {150, 200}
                set position of item "Applications" of container window to {350, 200}
                
                -- Close and reopen to apply changes
                close
                delay 1
                open
                
                -- Update the display
                update without registering applications
                delay 2
                
            end tell
        end tell
        
    on error errMsg
        display dialog "Error setting up DMG: " & errMsg
    end try
end run