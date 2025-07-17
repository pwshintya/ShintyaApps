/**
 * SettingsContext.jsx
 * 
 * Context Provider untuk manajemen pengaturan aplikasi dan preferensi pengguna.
 * Mengelola tema aplikasi (light/dark mode) dan menyimpan preferensi pengguna
 * secara persisten menggunakan AsyncStorage.
 * 
 * Fitur utama:
 * - Manajemen tema light/dark mode
 * - Mode enkripsi QR Code (encrypted/plain)
 * - Persistensi pengaturan dengan AsyncStorage
 * - Loading state untuk proses inisialisasi
 * - Error handling untuk operasi storage
 * - Default fallback values yang aman
 * 
 * @author Shintya Package Delivery System
 * @version 1.0.0
 */

import React, { createContext, useState, useContext, useEffect } from "react";
import AsyncStorage from "@react-native-async-storage/async-storage";

// Membuat React Context untuk pengaturan aplikasi
const SettingsContext = createContext();

/**
 * Hook kustom untuk mengakses SettingsContext
 * 
 * Menyediakan akses ke pengaturan aplikasi dari komponen manapun.
 * Jika context tidak tersedia (di luar SettingsProvider), akan mengembalikan
 * default values yang aman untuk mencegah error.
 * 
 * @returns {Object} Context pengaturan dengan properties:
 *   - theme: String tema saat ini ('light' atau 'dark')
 *   - encryptionMode: String mode enkripsi QR Code ('encrypted' atau 'plain')
 *   - loading: Boolean status loading saat inisialisasi
 *   - changeTheme: Function untuk mengubah tema
 *   - changeEncryptionMode: Function untuk mengubah mode enkripsi
 */
export const useSettings = () => {
  const context = useContext(SettingsContext);
  
  // Jika hook dipanggil di luar SettingsProvider, return default values
  if (!context) {
    return {
      theme: "light",            // Default ke tema light
      encryptionMode: "plain",   // Default ke mode plain
      loading: false,            // Tidak ada loading jika tidak ada context
      changeTheme: () => {},     // Function kosong sebagai fallback
      changeEncryptionMode: () => {}, // Function kosong sebagai fallback
    };
  }
  return context;
};

/**
 * SettingsProvider Component
 * 
 * Provider component yang membungkus aplikasi untuk menyediakan context pengaturan.
 * Mengelola state global untuk tema dan pengaturan aplikasi lainnya.
 * 
 * @param {Object} props
 * @param {ReactNode} props.children - Child components yang akan dibungkus provider
 */
export const SettingsProvider = ({ children }) => {
  // State untuk menyimpan tema saat ini ("light" atau "dark")
  const [theme, setTheme] = useState("light");
  
  // State untuk menyimpan mode enkripsi QR Code ("encrypted" atau "plain")
  const [encryptionMode, setEncryptionMode] = useState("plain");
  
  // State loading untuk proses inisialisasi pengaturan dari AsyncStorage
  const [loading, setLoading] = useState(true);

  /**
   * Memuat pengaturan aplikasi dari AsyncStorage
   * 
   * Function ini dipanggil saat komponen mount untuk memuat pengaturan
   * yang tersimpan secara persisten. Jika tidak ada pengaturan tersimpan
   * atau terjadi error, akan menggunakan tema "light" sebagai default.
   */
  const loadSettings = async () => {
    try {
      // Ambil tema yang tersimpan dari AsyncStorage
      const savedTheme = await AsyncStorage.getItem("app_theme");
      
      // Validasi nilai tema yang valid ("light" atau "dark")
      if (savedTheme && (savedTheme === "light" || savedTheme === "dark")) {
        setTheme(savedTheme);
      }
      
      // Ambil mode enkripsi yang tersimpan dari AsyncStorage
      const savedEncryptionMode = await AsyncStorage.getItem("encryption_mode");
      
      // Validasi nilai mode enkripsi yang valid ("encrypted" atau "plain")
      if (savedEncryptionMode && (savedEncryptionMode === "encrypted" || savedEncryptionMode === "plain")) {
        setEncryptionMode(savedEncryptionMode);
      }
      // Jika tidak ada tema tersimpan atau nilai tidak valid, gunakan default "light"
    } catch (error) {
      // Error handling jika gagal membaca dari AsyncStorage
      console.error("Error loading settings:", error);
      setTheme("light"); // Fallback ke tema light
      setEncryptionMode("plain"); // Fallback ke mode plain
    } finally {
      // Set loading selesai terlepas dari hasil operasi
      setLoading(false);
    }
  };

  /**
   * Mengubah tema aplikasi dan menyimpannya secara persisten
   * 
   * Function ini akan mengupdate state tema dan menyimpan pilihan
   * ke AsyncStorage agar tetap tersimpan setelah aplikasi ditutup.
   * 
   * @param {string} newTheme - Tema baru ("light" atau "dark")
   */
  const changeTheme = async (newTheme) => {
    try {
      // Validasi tema yang valid
      if (newTheme === "light" || newTheme === "dark") {
        // Update state tema untuk trigger re-render
        setTheme(newTheme);
        
        // Simpan pilihan tema ke AsyncStorage untuk persistensi
        await AsyncStorage.setItem("app_theme", newTheme);
      }
      // Jika tema tidak valid, abaikan permintaan
    } catch (error) {
      // Error handling jika gagal menyimpan ke AsyncStorage
      console.error("Error saving theme:", error);
      // Tetap update state meskipun gagal menyimpan ke storage
    }
  };

  /**
   * Mengubah mode enkripsi QR Code dan menyimpannya secara persisten
   * 
   * Function ini akan mengupdate state mode enkripsi dan menyimpan pilihan
   * ke AsyncStorage agar tetap tersimpan setelah aplikasi ditutup.
   * 
   * @param {string} newMode - Mode enkripsi baru ("encrypted" atau "plain")
   */
  const changeEncryptionMode = async (newMode) => {
    try {
      // Validasi mode enkripsi yang valid
      if (newMode === "encrypted" || newMode === "plain") {
        // Update state mode enkripsi untuk trigger re-render
        setEncryptionMode(newMode);
        
        // Simpan pilihan mode enkripsi ke AsyncStorage untuk persistensi
        await AsyncStorage.setItem("encryption_mode", newMode);
      }
      // Jika mode tidak valid, abaikan permintaan
    } catch (error) {
      // Error handling jika gagal menyimpan ke AsyncStorage
      console.error("Error saving encryption mode:", error);
      // Tetap update state meskipun gagal menyimpan ke storage
    }
  };

  /**
   * useEffect untuk inisialisasi pengaturan
   * 
   * Memuat pengaturan tersimpan dari AsyncStorage saat komponen mount.
   * 
   * Dependencies: [] (hanya dijalankan sekali saat mount)
   */
  useEffect(() => {
    loadSettings();
  }, []); // Empty dependency array - hanya dijalankan sekali saat mount

  // Object value yang akan disediakan oleh context provider
  const value = {
    theme: theme || "light",                           // Pastikan selalu ada nilai tema (fallback ke "light")
    encryptionMode: encryptionMode || "plain",         // Pastikan selalu ada nilai mode enkripsi (fallback ke "plain")
    loading,                                           // Status loading untuk proses inisialisasi
    changeTheme,                                      // Function untuk mengubah tema
    changeEncryptionMode,                             // Function untuk mengubah mode enkripsi
  };

  // Return Provider component dengan value context
  return (
    <SettingsContext.Provider value={value}>
      {children}
    </SettingsContext.Provider>
  );
};
