# CDLS.ps1 - PowerShell wrapper for CDLS DLL

# Define the C# code to P/Invoke the DLL functions
$source = @"
using System;
using System.Runtime.InteropServices;

public class CDLS_LIB {
    // Import the extern "C" functions from the DLL
    [DllImport("cdls_lib.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int CD([MarshalAs(UnmanagedType.LPStr)] string input = ".");

    [DllImport("cdls_lib.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void LS();

    [DllImport("cdls_lib.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int CDLS([MarshalAs(UnmanagedType.LPStr)] string input = ".");
}
"@

# Compile the C# code into an in-memory assembly
Add-Type -TypeDefinition $source -Language CSharp


# Function to change directory using CD
function Set-CDLSDirectory {
    param (
        [string]$Path = "."
    )
    try {
        $result = [CDLS_LIB]::CD($Path)
        if ($result -eq 0) {
            Write-Host "Directory changed successfully."
        } elseif ($result -eq 1) {
            Write-Warning "Invalid shell argument: $Path"
        } elseif ($result -eq 3) {
            Write-Host "Requested exit."
        } else {
            Write-Error "Unknown error occurred: $result"
        }
        return $result
    } catch {
        Write-Error "Error calling CD: $_"
        return -1
    }
}

# Function to list directory using LS
function Get-CDLSDirectory {
    try {
        [CDLS_LIB]::LS()
    } catch {
        Write-Error "Error calling LS: $_"
    }
}

# Function to change and list directory using CDLS
function Invoke-CDLS {
    param (
        [string]$Path = "."
    )
    try {
        $result = [CDLS_LIB]::CDLS($Path)
        if ($result -eq 0) {
            Write-Host "Operation completed successfully."
        } elseif ($result -eq 1) {
            Write-Warning "Invalid shell argument: $Path"
        } elseif ($result -eq 3) {
            Write-Host "Requested exit."
        } else {
            Write-Error "Unknown error occurred: $result"
        }
        return $result
    } catch {
        Write-Error "Error calling CDLS: $_"
        return -1
    }
}

# Export the functions for use in PowerShell
#Export-ModuleMember -Function Set-CDLSDirectory, Get-CDLSDirectory, Invoke-CDLS

# Example usage (commented out)
<#
# Change directory
Set-CDLSDirectory -Path ".."

# List current directory
Get-CDLSDirectory

# Change and list directory
Invoke-CDLS -Path ".3"
#>