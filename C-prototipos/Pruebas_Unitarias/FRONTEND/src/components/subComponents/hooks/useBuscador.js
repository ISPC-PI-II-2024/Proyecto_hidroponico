//=============================================
// HOOK BUSCADOR 
//=============================================

import { useState, useEffect } from "react";

export default function useBuscador(initialData, filterKeys) {
  const [searchTerm, setSearchTerm] = useState("");
  const [filteredData, setFilteredData] = useState(initialData);

  useEffect(() => {
    if (searchTerm) {
      const filtered = initialData.filter((item) =>
        filterKeys.some((key) => item[key]?.toLowerCase().includes(searchTerm))
      );
      setFilteredData(filtered);
    } else {
      setFilteredData(initialData);
    }
  }, [initialData, searchTerm, filterKeys]);

  const handleSearch = (event) => {
    const term = event.target.value.toLowerCase();
    setSearchTerm(term);
  };

  return {
    searchTerm,
    handleSearch,
    filteredData,
  };
}
