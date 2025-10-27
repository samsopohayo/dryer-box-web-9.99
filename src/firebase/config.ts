import { initializeApp } from "firebase/app";
import { getAuth } from "firebase/auth";
import { getDatabase } from "firebase/database";

const firebaseConfig = {
  apiKey: "AIzaSyCkXIB8zbRrR27K2kmt_S39pCKLQT5Wzls",
  authDomain: "dryer-box.firebaseapp.com",
  databaseURL:
    "https://dryer-box-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "dryer-box",
  storageBucket: "dryer-box.firebasestorage.app",
  messagingSenderId: "219907036660",
  appId: "1:219907036660:web:25e6663d87691ba4a0d5a1",
};

const app = initializeApp(firebaseConfig);
export const auth = getAuth(app);
export const database = getDatabase(app);
