/**
 * Service untuk sistem pairing kartu RFID dengan akun pengguna melalui ESP32.
 * 
 * Service ini mengelola proses pairing RFID card dengan akun user melalui komunikasi
 * real-time antara aplikasi mobile, Firebase, dan perangkat ESP32 dengan modul RFID RC522.
 * 
 * Alur Pairing RFID:
 * 1. User memulai pairing dari aplikasi mobile
 * 2. Sistem membuat session pairing di Firebase dengan timeout 30 detik
 * 3. ESP32 generate kode RFID 8-karakter hexadecimal secara random
 * 4. ESP32 menulis kode ke kartu RFID dan update Firebase
 * 5. Aplikasi mobile mendeteksi update dan menyimpan RFID ke profil user
 * 6. Session pairing ditutup otomatis
 * 
 * Hardware yang Digunakan:
 * - ESP32 WiFi Module
 * - RFID Reader RC522 (13.56MHz)
 * - RFID Cards/Tags ISO14443A
 * 
 * Protokol Komunikasi:
 * - WiFi connection ke Firebase Firestore
 * - Real-time synchronization dengan onSnapshot
 * - Timeout mechanism untuk keamanan session
 * 
 * Format Kode RFID:
 * - 8 karakter hexadecimal (contoh: A1B2C3D4)
 * - Generated secara random oleh ESP32
 * - Unik untuk setiap user
 * 
 * @author Package Tracking System
 * @version 1.0.0
 */

import { 
  doc, 
  setDoc, 
  getDoc, 
  onSnapshot, 
  updateDoc
} from 'firebase/firestore';
import { db } from './firebase';
import { updateUserRFID } from './userService';

// Konstanta untuk konfigurasi collection pairing RFID
const PAIRING_COLLECTION = 'rfid_pairing';
const PAIRING_DOC_ID = 'current_session';

/**
 * Memulai proses pairing RFID card untuk user yang sedang login.
 * 
 * Fungsi ini menginisialisasi session pairing RFID dengan timeout 30 detik.
 * ESP32 akan mendeteksi session aktif dan mulai proses generate + write RFID code.
 * 
 * Alur Proses Pairing:
 * 1. Validasi koneksi Firebase
 * 2. Buat session pairing dengan status 'waiting'
 * 3. Set auto-timeout 30 detik untuk keamanan
 * 4. ESP32 polling Firebase dan deteksi session aktif
 * 5. ESP32 generate kode RFID random (8 hex chars)
 * 6. ESP32 write kode ke kartu RFID fisik
 * 7. ESP32 update Firebase dengan kode yang di-generate
 * 
 * Keamanan Session:
 * - Maximum 30 detik per session
 * - Auto-cancel jika timeout
 * - Hanya satu session aktif per waktu
 * - Validasi userId untuk mencegah konflik
 * 
 * @async
 * @function startPairing
 * @param {string} userId - ID user yang meminta pairing RFID
 * @returns {Promise<Object>} Response object dengan format:
 *   - success: boolean - Status keberhasilan inisialisasi
 *   - error: string - Pesan error jika inisialisasi gagal
 * 
 * @example
 * const result = await startPairing('user123');
 * if (result.success) {
 *   console.log('Session pairing dimulai, tunggu 30 detik');
 *   // Setup listener untuk mendeteksi kode RFID dari ESP32
 * }
 */
export const startPairing = async (userId) => {
  try {
    // Validasi koneksi Firebase sebelum memulai session
    if (!db) {
      throw new Error('Firestore belum diinisialisasi');
    }

    // Data session pairing untuk komunikasi dengan ESP32
    const pairingData = {
      isActive: true,                      // Flag session aktif untuk ESP32
      userId: userId,                      // ID user yang request pairing
      startTime: new Date().toISOString(), // Timestamp mulai session
      rfidCode: '',                        // Kosong, akan diisi oleh ESP32
      status: 'waiting',                   // Status menunggu ESP32 generate kode
      cancelledTime: '',                   // Timestamp jika dibatalkan
      receivedTime: ''                     // Timestamp saat kode diterima
    };

    // Simpan session ke Firebase untuk komunikasi dengan ESP32
    await setDoc(doc(db, PAIRING_COLLECTION, PAIRING_DOC_ID), pairingData);
    
    // Timeout otomatis 30 detik untuk keamanan session
    setTimeout(async () => {
      try {
        // Cek apakah session masih aktif setelah 30 detik
        const currentDoc = await getDoc(doc(db, PAIRING_COLLECTION, PAIRING_DOC_ID));
        if (currentDoc.exists() && 
            currentDoc.data().userId === userId && 
            currentDoc.data().isActive) {
          // Auto-cancel session jika masih aktif (timeout)
          await cancelPairing();
          console.log('Pairing session auto-cancelled setelah 30 detik');
        }
      } catch (error) {
        console.error('Error auto-canceling pairing:', error);
      }
    }, 30000); // 30 detik timeout

    return { success: true };
  } catch (error) {
    console.error('Error starting pairing:', error);
    return { success: false, error: error.message };
  }
};

/**
 * Membatalkan session pairing RFID yang sedang berlangsung.
 * 
 * Fungsi ini akan menghentikan proses pairing dan membersihkan session data
 * di Firebase. ESP32 akan mendeteksi perubahan ini dan menghentikan proses
 * generate/write RFID code.
 * 
 * Skenario Pembatalan:
 * - User membatalkan secara manual dari aplikasi
 * - Auto-timeout setelah 30 detik
 * - Error dalam proses pairing
 * - System shutdown atau maintenance
 * 
 * Efek Pembatalan:
 * - ESP32 menghentikan proses RFID
 * - Session data dihapus dari Firebase
 * - UI aplikasi kembali ke state normal
 * - Listener onSnapshot akan menerima update
 * 
 * @async
 * @function cancelPairing
 * @returns {Promise<Object>} Response object dengan format:
 *   - success: boolean - Status keberhasilan pembatalan
 *   - error: string - Pesan error jika pembatalan gagal
 * 
 * @example
 * const result = await cancelPairing();
 * if (result.success) {
 *   console.log('Session pairing berhasil dibatalkan');
 * }
 */
export const cancelPairing = async () => {
  try {
    // Validasi koneksi Firebase
    if (!db) {
      throw new Error('Firestore belum diinisialisasi');
    }

    // Referensi dokumen session pairing
    const docRef = doc(db, PAIRING_COLLECTION, PAIRING_DOC_ID);
    
    // Reset semua data session untuk menghentikan proses ESP32
    await setDoc(docRef, {
      isActive: false,      // Flag non-aktif untuk ESP32
      userId: '',           // Clear user ID
      startTime: '',        // Clear timestamp mulai
      rfidCode: '',         // Clear kode RFID
      status: '',           // Clear status session
      cancelledTime: '',    // Clear timestamp pembatalan
      receivedTime: ''      // Clear timestamp terima kode
    });

    return { success: true };
  } catch (error) {
    console.error('Error canceling pairing:', error);
    return { success: false, error: error.message };
  }
};

/**
 * Mengambil status session pairing RFID yang sedang berlangsung.
 * 
 * Fungsi ini digunakan untuk mengecek apakah ada session pairing aktif
 * dan mendapatkan detail session seperti userId, waktu mulai, dan status.
 * 
 * Kegunaan:
 * - Validasi session sebelum memulai pairing baru
 * - Monitoring status pairing untuk UI
 * - Debugging proses pairing
 * - Checking timeout session
 * 
 * @async
 * @function getPairingStatus
 * @returns {Promise<Object|null>} Data session pairing jika aktif, null jika tidak ada
 *   Object berisi:
 *   - isActive: boolean - Status session aktif
 *   - userId: string - ID user yang sedang pairing
 *   - startTime: string - Timestamp mulai session
 *   - rfidCode: string - Kode RFID (kosong jika belum di-generate)
 *   - status: string - Status proses ('waiting', 'received', dll)
 * 
 * @example
 * const status = await getPairingStatus();
 * if (status) {
 *   console.log('Ada session aktif untuk user:', status.userId);
 *   console.log('Status:', status.status);
 * } else {
 *   console.log('Tidak ada session pairing aktif');
 * }
 */
export const getPairingStatus = async () => {
  try {
    // Return null jika Firebase belum terhubung
    if (!db) {
      return null;
    }

    // Ambil dokumen session pairing dari Firebase
    const docRef = doc(db, PAIRING_COLLECTION, PAIRING_DOC_ID);
    const docSnap = await getDoc(docRef);

    if (docSnap.exists()) {
      const data = docSnap.data();
      // Return data hanya jika session masih aktif
      if (data.isActive) {
        return data;
      }
    }
    
    // Return null jika tidak ada session aktif
    return null;
  } catch (error) {
    console.error('Error getting pairing status:', error);
    return null;
  }
};

/**
 * Listener real-time untuk monitoring proses pairing RFID dengan ESP32.
 * 
 * Fungsi ini membuat subscription Firebase yang mendeteksi saat ESP32 berhasil
 * generate dan write kode RFID ke kartu. Ketika kode RFID diterima, akan
 * otomatis disimpan ke profil user dan session pairing ditutup.
 * 
 * Alur Real-time Communication:
 * 1. Aplikasi mobile setup listener ini setelah startPairing()
 * 2. ESP32 polling Firebase dan deteksi session aktif
 * 3. ESP32 generate kode RFID 8-karakter random
 * 4. ESP32 write kode ke kartu RFID fisik menggunakan RC522
 * 5. ESP32 update Firebase dengan kode yang berhasil ditulis
 * 6. Firebase trigger onSnapshot listener di aplikasi
 * 7. Callback dipanggil dengan kode RFID baru
 * 8. Kode RFID disimpan ke profil user otomatis
 * 9. Session pairing ditutup
 * 
 * Validasi Data:
 * - Cek rfidCode tidak kosong (ESP32 sudah generate)
 * - Cek userId sesuai dengan session
 * - Validasi format kode RFID sebelum menyimpan
 * 
 * @function listenToPairingData
 * @param {Function} callback - Function yang dipanggil saat ada update
 *   Callback menerima parameter:
 *   - success: boolean - Status keberhasilan pairing
 *   - rfidCode: string - Kode RFID yang berhasil di-generate (jika berhasil)
 *   - userId: string - ID user yang melakukan pairing (jika berhasil)
 *   - error: string - Pesan error (jika gagal)
 * @returns {Function|null} Unsubscribe function atau null jika gagal setup
 * 
 * @example
 * const unsubscribe = listenToPairingData((result) => {
 *   if (result.success) {
 *     console.log('RFID berhasil dipasangkan:', result.rfidCode);
 *     showSuccessMessage('Kartu RFID berhasil dipasangkan!');
 *   } else {
 *     console.error('Pairing gagal:', result.error);
 *     showErrorMessage(result.error);
 *   }
 * });
 * 
 * // Cleanup saat component unmount
 * // unsubscribe();
 */
export const listenToPairingData = (callback) => {
  // Validasi koneksi Firebase sebelum setup listener
  if (!db) {
    console.warn('Firestore belum diinisialisasi, skip listener');
    return null;
  }

  try {
    // Referensi dokumen session pairing untuk real-time monitoring
    const docRef = doc(db, PAIRING_COLLECTION, PAIRING_DOC_ID);
    
    // Setup listener Firebase untuk deteksi update dari ESP32
    const unsubscribe = onSnapshot(docRef, async (doc) => {
      if (doc.exists()) {
        const data = doc.data();
        
        // Cek apakah ESP32 sudah berhasil generate dan write RFID code
        if (data.rfidCode && data.rfidCode !== '' && 
            data.userId && data.userId !== '') {
          try {
            // Simpan kode RFID ke profil user di Firebase
            const result = await updateUserRFID(data.userId, data.rfidCode);
            
            if (result.success) {
              // Berhasil simpan RFID, tutup session pairing
              await cancelPairing();
              
              // Kirim konfirmasi sukses ke callback
              callback({ 
                success: true, 
                rfidCode: data.rfidCode, 
                userId: data.userId 
              });
            } else {
              // Gagal simpan RFID ke profil user
              callback({ 
                success: false, 
                error: result.error 
              });
            }
          } catch (error) {
            console.error('Error saving RFID to user:', error);
            callback({ 
              success: false, 
              error: error.message 
            });
          }
        }
      }
    });

    // Return unsubscribe function untuk cleanup
    return unsubscribe;
  } catch (error) {
    console.error('Error setting up pairing listener:', error);
    return null;
  }
};

/**
 * Update kode RFID dari ESP32 ke session pairing Firebase.
 * 
 * Fungsi ini dipanggil oleh ESP32 setelah berhasil generate dan write
 * kode RFID ke kartu fisik. ESP32 akan update Firebase dengan kode
 * yang berhasil ditulis untuk disinkronisasi dengan aplikasi mobile.
 * 
 * Protokol ESP32 Communication:
 * 1. ESP32 deteksi session pairing aktif di Firebase
 * 2. ESP32 generate kode RFID 8-karakter hexadecimal random
 * 3. ESP32 write kode ke kartu RFID menggunakan modul RC522
 * 4. Jika write berhasil, ESP32 panggil fungsi ini
 * 5. Firebase update memicu listener di aplikasi mobile
 * 6. Aplikasi mobile otomatis simpan RFID ke profil user
 * 
 * Validasi Session:
 * - Cek session masih aktif (isActive = true)
 * - Validasi format kode RFID (8 hex chars)
 * - Update status dan timestamp untuk audit trail
 * 
 * @async
 * @function updateRFIDCode
 * @param {string} rfidCode - Kode RFID 8-karakter yang di-generate ESP32
 * @returns {Promise<Object>} Response object dengan format:
 *   - success: boolean - Status keberhasilan update
 *   - error: string - Pesan error jika update gagal
 * 
 * @example
 * // Dipanggil oleh ESP32 setelah berhasil write RFID
 * const result = await updateRFIDCode('A1B2C3D4');
 * if (result.success) {
 *   console.log('Kode RFID berhasil diupdate ke Firebase');
 * }
 */
export const updateRFIDCode = async (rfidCode) => {
  try {
    // Validasi koneksi Firebase
    if (!db) {
      throw new Error('Firestore belum diinisialisasi');
    }

    // Ambil session pairing untuk validasi
    const docRef = doc(db, PAIRING_COLLECTION, PAIRING_DOC_ID);
    const docSnap = await getDoc(docRef);

    // Pastikan session masih aktif sebelum update
    if (docSnap.exists() && docSnap.data().isActive) {
      // Update session dengan kode RFID dari ESP32
      await updateDoc(docRef, {
        rfidCode: rfidCode,              // Kode RFID yang berhasil ditulis
        status: 'received',             // Status berubah ke 'received'
        receivedTime: new Date()        // Timestamp saat kode diterima
      });
      
      return { success: true };
    }

    // Session tidak aktif atau tidak ditemukan
    return { 
      success: false, 
      error: 'Tidak ada session pairing yang aktif' 
    };
  } catch (error) {
    console.error('Error updating RFID code:', error);
    return { 
      success: false, 
      error: error.message 
    };
  }
};